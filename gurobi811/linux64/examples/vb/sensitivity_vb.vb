' Copyright 2019, Gurobi Optimization, LLC

' A simple sensitivity analysis example which reads a MIP model
' from a file and solves it. Then each binary variable is set
' to 1-X, where X is its value in the optimal solution, and
' the impact on the objective function value is reported.

Imports System
Imports Gurobi

Class sensitivity_vb
    Shared Sub Main(args As String())
        If args.Length < 1 Then
            Console.Out.WriteLine("Usage: sensitivity_vb filename")
            Return
        End If

        Try

            ' Create environment

            Dim env As New GRBEnv()

            ' Read and solve model

            Dim model As New GRBModel(env, args(0))

            If model.IsMIP = 0 Then
                Console.WriteLine("Model is not a MIP")
                Return
            End If

            model.Optimize()

            If model.Status <> GRB.Status.OPTIMAL Then
                Console.WriteLine("Optimization ended with status " & _
                                  model.Status)
                Return
            End If

            ' Store the optimal solution

            Dim origObjVal As Double = model.ObjVal
            Dim vars As GRBVar() = model.GetVars()
            Dim origX As Double() = model.Get(GRB.DoubleAttr.X, vars)

            ' Disable solver output for subsequent solves

            model.Parameters.OutputFlag = 0

            ' Iterate through unfixed, binary variables in model

            For i As Integer = 0 To vars.Length - 1
                Dim v As GRBVar = vars(i)
                Dim vType As Char = v.VType

                If v.LB = 0 AndAlso _
                   v.UB = 1 AndAlso _
                   (vType = GRB.BINARY OrElse vType = GRB.INTEGER) Then

                    ' Set variable to 1-X, where X is its value in optimal solution

                    If origX(i) < 0.5 Then
                        v.LB = 1.0
                        v.Start = 1.0
                    Else
                        v.UB = 0.0
                        v.Start = 0.0
                    End If

                    ' Update MIP start for the other variables

                    For j As Integer = 0 To vars.Length - 1
                        If j <> i Then
                            vars(j).Start = origX(j)
                        End If
                    Next

                    ' Solve for new value and capture sensitivity information

                    model.Optimize()

                    If model.Status = GRB.Status.OPTIMAL Then
                        Console.WriteLine("Objective sensitivity for variable " & _
                                          v.VarName & " is " & (model.ObjVal - origObjVal))
                    Else
                        Console.WriteLine("Objective sensitivity for variable " & _
                                          v.VarName & " is infinite")
                    End If

                    ' Restore the original variable bounds

                    v.LB = 0.0
                    v.UB = 1.0
                End If
            Next

            ' Dispose of model and environment

            model.Dispose()

            env.Dispose()
        Catch e As GRBException
            Console.WriteLine("Error code: " + e.ErrorCode)
            Console.WriteLine(e.Message)
            Console.WriteLine(e.StackTrace)
        End Try
    End Sub
End Class

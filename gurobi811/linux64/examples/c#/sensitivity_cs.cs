/* Copyright 2019, Gurobi Optimization, LLC */

/* A simple sensitivity analysis example which reads a MIP model
   from a file and solves it. Then each binary variable is set
   to 1-X, where X is its value in the optimal solution, and
   the impact on the objective function value is reported.
*/

using System;
using Gurobi;

class sensitivity_cs
{
  static void Main(string[] args)
  {
    if (args.Length < 1) {
      Console.Out.WriteLine("Usage: sensitivity_cs filename");
      return;
    }

    try {

      // Create environment

      GRBEnv env = new GRBEnv();

      // Read and solve model

      GRBModel model = new GRBModel(env, args[0]);

      if (model.IsMIP == 0) {
        Console.WriteLine("Model is not a MIP");
        return;
      }

      model.Optimize();

      if (model.Status != GRB.Status.OPTIMAL) {
        Console.WriteLine("Optimization ended with status " + model.Status);
        return;
      }

      // Store the optimal solution

      double   origObjVal = model.ObjVal;
      GRBVar[] vars       = model.GetVars();
      double[] origX      = model.Get(GRB.DoubleAttr.X, vars);

      // Disable solver output for subsequent solves

      model.Parameters.OutputFlag = 0;

      // Iterate through unfixed, binary variables in model

      for (int i = 0; i < vars.Length; i++) {
        GRBVar v     = vars[i];
        char   vType = v.VType;

        if (v.LB == 0 && v.UB == 1
            && (vType == GRB.BINARY || vType == GRB.INTEGER)) {

          // Set variable to 1-X, where X is its value in optimal solution

          if (origX[i] < 0.5) {
            v.LB = 1.0;
            v.Start = 1.0;
          } else {
            v.UB = 0.0;
            v.Start = 0.0;
          }

          // Update MIP start for the other variables

          for (int j = 0; j < vars.Length; j++) {
            if (j != i) {
              vars[j].Start = origX[j];
            }
          }

          // Solve for new value and capture sensitivity information

          model.Optimize();

          if (model.Status == GRB.Status.OPTIMAL) {
            Console.WriteLine("Objective sensitivity for variable "
                + v.VarName + " is " + (model.ObjVal - origObjVal));
          } else {
            Console.WriteLine("Objective sensitivity for variable "
                + v.VarName + " is infinite");
          }

          // Restore the original variable bounds

          v.LB = 0.0;
          v.UB = 1.0;
        }
      }

      // Dispose of model and environment

      model.Dispose();
      env.Dispose();

    } catch (GRBException e) {
      Console.WriteLine("Error code: " + e.ErrorCode);
      Console.WriteLine(e.Message);
      Console.WriteLine(e.StackTrace);
    }
  }
}

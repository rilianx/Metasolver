%   gurobi_feasrelax
%
%    feasrel = gurobi_feasrelax(model, relaxobjtype, minrelax, penalties)
%    feasrel = gurobi_feasrelax(model, relaxobjtype, minrelax, penalties, params)
%    feasrel = gurobi_feasrelax(model, relaxobjtype, minrelax, penalties, params, env)
%
%    This function computes a feasibility relaxation for the input model
%    argument. The feasibility relaxation is a model that, when solved,
%    minimizes the amount by which the solution violates the bounds and
%    linear constraints of the original model. You must provide a penalty to
%    associate with relaxing each individual bound or constraint (through
%    the penalties argument). These penalties are interpreted as weights for
%    the various ways of measuring violations, depending on the value of the
%    relaxobjtype argument.
%
%    Input arguments:
%
%    model: The model struct must contain a valid Gurobi model. See the model
%      argument section in the reference documentation for more information.
%
%    relaxobjtype: An integer value that controls the way violations are
%      measured. The following values for relaxobjtype are allowed:
%        0: The feasibility relaxation minimizes the sum of the weighted
%           magnitudes of the bound and constraint violations.
%        1: The feasibility relaxation minimize the weighted sum of the squares
%           of the bound and constraint violations.
%        2: The feasibility relaxation minimizes the weighted count of bound and
%           constraint violations.
%
%      In all cases, the weights are taken from penalties.lb, penalties.ub and
%      penalties.rhs. You can provide the special penalty value Inf to
%      indicate that the corresponding bound or constraint cannot be relaxed.
%
%    minrelax: A boolean that controls the type of feasibility relaxation.
%      If minrelax=false, optimizing the returned model gives a solution that
%        minimizes the cost of the violation.
%      If minrelax=true, optimizing the returned model finds a solution that
%        minimizes the original objective, but only from among those solutions
%        that minimize the cost of the violation. Note that in this case
%        gurobi_feasrelax must solve an optimization problem to find the minimum
%        possible relaxation, which can be quite expensive.
%
%    penalties: The penalties argument is a struct array, having the following
%      optional fields (default: all Inf):
%        lb:  Penalty for violating each lower bound.
%        ub:  Penalty for violating each upper bound.
%        rhs: Penalty for violating each constraint.
%
%    To give an example, if a constraint with penalties.rhs value p is
%    violated by 2.0, it would contribute 2*p to the feasibility relaxation
%    objective for relaxobjtype=0, 2*2*p for relaxobjtype=1, and p for
%    relaxobjtype=2.
%
%    params (optional): The params struct, when provided, contains a list of
%      Gurobi parameters.  See the params argument section in the reference
%      documentation for more information.
%
%    env (optional): The env struct, when provided, allows you to use
%      Gurobi Compute Server or Gurobi Instant Cloud. See the env argument
%      section in the reference documentation for more information.
%
%    Output arguments:
%
%    feasrel: A struct containing two fields:
%      feasrel.model: A struct.  Its fields define a model for the feasibility
%        relaxation.
%      feasrel.feasobj: A scalar. If minrelax==true this is the relaxation
%        problem objective value, and 0.0 otherwise.
%
%    Example usage:
%      model = gurobi_read('stein9.mps');
%      penalties.lb = ones(length(model.lb),1);
%      penalties.ub = ones(length(model.ub),1);
%      penalties.rhs = ones(length(model.rhs),1);
%      feasrelaxresult = gurobi_feasrelax(model, 0, false, penalties);
%
%    All details on input and output arguments, and general
%    information on the Gurobi Optimizer Matlab Interface, are given in
%    <a href="matlab:web(fullfile(fileparts(which('gurobi')), 'html', 'index.html'))">included html documentation</a>, and on-line at the <a href="matlab:web('www.gurobi.com/documentation/8.0/refman/matlab_api_overview.html')">Gurobi Documentation</a> page.
%
% Copyright 2019, Gurobi Optimization, LLC

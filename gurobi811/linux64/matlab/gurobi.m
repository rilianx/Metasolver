%   gurobi
%
%    result = gurobi(model)
%    result = gurobi(model, params)
%    result = gurobi(model, params, env)
%
%    This function optimizes the given model. The algorithm used for the
%    optimization depends on the model type (simplex or barrier for a
%    continuous model; branch-and-cut for a MIP model). Upon successful
%    completion it will return a struct containing solution information.
%
%    Input arguments:
%
%    model: The model struct must contain a valid Gurobi model. See the model
%      argument section in the reference documentation for more information.
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
%    result: A struct containing solution information, such as optimization
%      status, objective value, and solution vector.  See the result argument
%      section in the reference documentation for more information.
%
%    Example usage:
%
%      result = gurobi(model, params);
%      if strcmp(result.status, 'OPTIMAL');
%          fprintf('Optimal objective: %e\n', result.objval);
%          disp(result.x)
%      else
%          fprintf('Optimization returned status: %s\n', result.status);
%      end
%
%    All details on input and output arguments, and general
%    information on the Gurobi Optimizer Matlab Interface, are given in
%    <a href="matlab:web(fullfile(fileparts(which('gurobi')), 'html', 'index.html'))">included html documentation</a>, and on-line at the <a href="matlab:web('www.gurobi.com/documentation/8.0/refman/matlab_api_overview.html')">Gurobi Documentation</a> page.
%
% Copyright 2019, Gurobi Optimization, LLC

%   gurobi_relax
%
%    relaxed = gurobi_relax(model)
%    relaxed = gurobi_relax(model, env)
%
%    Create the relaxation of a MIP model. Transforms integer variables into
%    continuous variables, and removes SOS and general constraints.
%
%    Input arguments:
%
%    model: The model struct must contain a valid Gurobi model. See the model
%      argument section in the reference documentation for more information.
%
%    env (optional): The env struct, when provided, allows you to use
%      Gurobi Compute Server or Gurobi Instant Cloud. See the env argument
%      section in the reference documentation for more information.
%
%    Output arguments:
%
%    relaxed: A model struct, as described in the model parameter section of
%      the reference manual.
%
%    Example usage:
%
%      model = gurobi_read('stein9.mps');
%      relaxed = gurobi_relax(model);
%
%
%    All details on input and output arguments, and general
%    information on the Gurobi Optimizer Matlab Interface, are given in
%    <a href="matlab:web(fullfile(fileparts(which('gurobi')), 'html', 'index.html'))">included html documentation</a>, and on-line at the <a href="matlab:web('www.gurobi.com/documentation/8.0/refman/matlab_api_overview.html')">Gurobi Documentation</a> page.
%
% Copyright 2019, Gurobi Optimization, LLC

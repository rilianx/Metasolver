%   gurobi_iis
%
%    iis = gurobi_iis(model)
%    iis = gurobi_iis(model, params)
%    iis = gurobi_iis(model, params, env)
%
%    Compute an Irreducible Inconsistent Subsystem (IIS).
%
%    An IIS is a subset of the constraints and variable bounds with the
%    following properties:
%      * the subsystem represented by the IIS is infeasible, and
%      * if any of the constraints or bounds of the IIS is removed, the
%        subsystem becomes feasible.
%
%    Note that an infeasible model may have multiple IISs. The one returned
%    by Gurobi is not necessarily the one with minimum cardinality; there
%    may exist others with fewer constraints or bounds.
%
%    You can obtain information about the outcome of the IIS computation
%    from the returned IIS result (described below). Note that this method
%    can be used to compute IISs for both continuous and MIP models.
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
%    iis: A struct describing the IIS found.
%
%      iis.minimal: A logical scalar that indicates whether the computed
%        IIS is minimal. It will normally be true, but it may be false if the
%        IIS computation was stopped early (e.g due to a time limit).
%
%      iis.Arows: A logical vector that indicates whether a linear constraint
%        appears in the computed IIS.
%
%      iis.lb: A logical vector that indicates whether a lower bound appears
%        in the computed IIS.
%
%      iis.ub: A logical vector that indicates whether a upper bound appears
%        in the computed IIS.
%
%    If your model contains SOS constraints, quadratic constraints, or general
%    constraints, the IIS struct will contain corresponding logical vectors to
%    indicate their membership in the IIS.
%
%    Example usage:
%      model = gurobi_read('examples/data/klein1.mps');
%      iis = gurobi_iis(model);
%
%    All details on input and output arguments, and general
%    information on the Gurobi Optimizer Matlab Interface, are given in
%    <a href="matlab:web(fullfile(fileparts(which('gurobi')), 'html', 'index.html'))">included html documentation</a>, and on-line at the <a href="matlab:web('www.gurobi.com/documentation/8.0/refman/matlab_api_overview.html')">Gurobi Documentation</a> page.
%
% Copyright 2019, Gurobi Optimization, LLC

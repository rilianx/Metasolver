%   gurobi_write
%
%    gurobi_write(model, filename)
%    gurobi_write(model, filename, env)
%
%    Writes a model to a file.
%
%    Input arguments:
%
%    model: The model struct must contain a valid Gurobi model. See the  model
%      argument section in the reference documentation for more information.
%
%    filename: Name of the file to write. Note that the type of the file is
%      encoded in the file name suffix. The filename suffix should be one of
%      .mps, .rew, .lp, .rlp, or .ilp, to indicate the desired file format
%      (see the file formats section for details on Gurobi file formats). The
%      files can be compressed, so additional suffixes of .gz, .bz2, .zip, or
%      .7z are accepted.
%
%    env (optional): The env struct, when provided, allows you to use
%      Gurobi Compute Server or Gurobi Instant Cloud. See the env argument
%      section in the reference documentation for more information.
%
%    Example usage:
%      model.A          = sparse([1 2 3; 1 1 0]);
%      model.obj        = [1 1 2];
%      model.modelsense = 'max';
%      model.rhs        = [4; 1];
%      model.sense      = '<>';
%      gurobi_write(model, 'mymodel.mps');
%      gurobi_write(model, 'mymodel.lp');
%      gurobi_write(model, 'mymodel.mps.bz2');
%
%
%    All details on input and output arguments, and general
%    information on the Gurobi Optimizer Matlab Interface, are given in
%    <a href="matlab:web(fullfile(fileparts(which('gurobi')), 'html', 'index.html'))">included html documentation</a>, and on-line at the <a href="matlab:web('www.gurobi.com/documentation/8.0/refman/matlab_api_overview.html')">Gurobi Documentation</a> page.
%
%
% Copyright 2019, Gurobi Optimization, LLC

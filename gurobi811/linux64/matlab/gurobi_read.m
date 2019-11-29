%   gurobi_read
%
%    model = gurobi_read(filename)
%    model = gurobi_read(filename, env)
%
%    Reads a model from a file.
%
%    Input arguments:
%
%    filename: Name of the file to read. Note that the type of the file is
%      encoded in the file name suffix. The filename suffix should be one of
%      .mps, .rew, .lp, .rlp, .ilp, or .opb (see the file formats section for
%      details on Gurobi file formats). The files can be compressed, so
%      additional suffixes of .gz, .bz2, .zip, or .7z are accepted.
%
%    env (optional): The env struct, when provided, allows you to use
%      Gurobi Compute Server or Gurobi Instant Cloud. See the env argument
%      section in the reference documentation for more information.
%
%    Output arguments:
%
%    model: A struct, representing the model read.   See the model argument
%      section in the reference documentation for more information.
%
%    Example usage:
%    model = gurobi_read('stein9.mps');
%    result = gurobi(model);
%
%    All details on input and output arguments, and general
%    information on the Gurobi Optimizer Matlab Interface, are given in
%    <a href="matlab:web(fullfile(fileparts(which('gurobi')), 'html', 'index.html'))">included html documentation</a>, and on-line at the <a href="matlab:web('www.gurobi.com/documentation/8.0/refman/matlab_api_overview.html')">Gurobi Documentation</a> page.
%
% Copyright 2019, Gurobi Optimization, LLC

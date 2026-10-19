% clears the command window
clearvars; close all; clc;

%----------FLIGHT 1----------
% checks if the file that will be parsed can be found or not
if ~isfile('datalog1.csv')
    error('datalog1.csv not found in the current folder.');
end

% force every column numeric at import. junk cells like "ERR" become
% NaN instead of turning the whole column into text
opts = detectImportOptions('datalog1.csv');
opts = setvartype(opts, 'double');
T = readtable('datalog1.csv', opts);

% make sure Time exists before touching T.Time
if ~ismember('Time', T.Properties.VariableNames)
    error('No Time column. Found: %s', strjoin(T.Properties.VariableNames, ', '));
end

% check if whether Time has any real values at all
if ~any(isfinite(T.Time))
    error('Time column has no valid values.');
end

% checks to see if time goes backwards or not
if any(diff(T.Time(isfinite(T.Time))) <= 0)
    warning('Timestamps repeat or go backwards.');
end

% {column name in CSV, display label, unit, min/max}
signals = {
    'Longitude','Longitude','deg', [-87, -86];
    'Latitude','Latitude','deg', [34, 35];
    'Altitude','Altitude','m', [100, 3000];
    'Pressure','Pressure','hPa', [700, 1100];
    'Temperature','Temperature','°C', [-10, 40];
    'Roll','Roll','deg', [-30, 30];
    'Pitch','Pitch','deg', [-10, 15];
    'Yaw','Yaw','deg', [0, 360];
    'Ax','Acceleration (x)','m/s^2', [-20, 20];
    'Ay','Acceleration (y)','m/s^2', [-20, 20];
    'Az','Acceleration (z)','m/s^2', [-20, 20];
    'Speed','Speed','km/h', [0, Inf]
};

t_ms = T.Time; % retrieves data regarding time in milliseconds
tvalid = t_ms(isfinite(t_ms)); %retrieves valid data regarding time
t_s = (t_ms - min(tvalid)) / 1000; %converts to seconds

% flight summary
fprintf('Flight 1 Summary\n');
% print flight time
fprintf('Flight time: %.2f s\n', (max(tvalid) - min(tvalid)) / 1000);

n = size(signals, 1);
% loops through each row in the array
for i = 1:n
    % retrieves data for the current row in the array
    col   = signals{i, 1};
    label = signals{i, 2};
    unit  = signals{i, 3};
    limits = signals{i, 4};

    % skip only if the column header isn't in the CSV
    if ~ismember(col, T.Properties.VariableNames)
        warning('Column %s not found, skipping.', col);
        continue;
    end

    % retrieves the actual data for the row from the csv file
    y = T.(col);

    % checks that the data exists and is not physically impossible
    present = isfinite(t_ms) & isfinite(y); % not missing and not infinite
    corrupt = present & (y < limits(1) | y > limits(2));
    keep  = present & ~corrupt; % passed both checks

    % counts the total number of missing and corrupt
    missingSum = sum(~present);
    corruptSum = sum(corrupt);

    % stores cleaned samples for summary statistics and plotting
    yi = y(keep);

    % checks to see if the data is empty or not
    if isempty(yi)
        warning('%s has no valid samples, skipping.', label);
        continue;
    end

    % prints the summary for the current column
    fprintf('%s Max: %f  Min: %f  (missing %d, corrupt %d)\n', ...
            label, max(yi), min(yi), missingSum, corruptSum);

    % blank bad samples with NaN instead of deleting them,
    % so dropouts show as gaps instead of straight lines across them
    yplot = y;
    yplot(~keep) = NaN;

    % plotting the graph for current column
    figure;
    plot(t_s, yplot, 'b.-', 'LineWidth', 2);
    xlabel('Time (s)');
    ylabel(sprintf('%s (%s)', label, unit));
    title(sprintf('%s as a Function of Time', label));
    grid on;
end
%----------------------------

%----------FLIGHT 2----------
% checks if the file that will be parsed can be found or not
if ~isfile('datalog2.csv')
    error('datalog2.csv not found in the current folder.');
end

% force every column numeric at import. junk cells like "ERR" become
% NaN instead of turning the whole column into text
opts = detectImportOptions('datalog2.csv');
opts = setvartype(opts, 'double');
T = readtable('datalog2.csv', opts);

% make sure Time exists before touching T.Time
if ~ismember('Time', T.Properties.VariableNames)
    error('No Time column. Found: %s', strjoin(T.Properties.VariableNames, ', '));
end

% check if whether Time has any real values at all
if ~any(isfinite(T.Time))
    error('Time column has no valid values.');
end

% checks to see if time goes backwards or not
if any(diff(T.Time(isfinite(T.Time))) <= 0)
    warning('Timestamps repeat or go backwards.');
end

% {column name in CSV, display label, unit, min/max}
signals = {
    'Longitude','Longitude','deg', [-87, -86];
    'Latitude','Latitude','deg', [34, 35];
    'Altitude','Altitude','m', [100, 3000];
    'Pressure','Pressure','hPa', [700, 1100];
    'Temperature','Temperature','°C', [-10, 40];
    'Roll','Roll','deg', [-30, 30];
    'Pitch','Pitch','deg', [-10, 15];
    'Yaw','Yaw','deg', [0, 360];
    'Ax','Acceleration (x)','m/s^2', [-20, 20];
    'Ay','Acceleration (y)','m/s^2', [-20, 20];
    'Az','Acceleration (z)','m/s^2', [-20, 20];
    'Speed','Speed','km/h', [0, Inf]
};

t_ms = T.Time; % retrieves data regarding time in milliseconds
tvalid = t_ms(isfinite(t_ms)); %retrieves valid data regarding time
t_s = (t_ms - min(tvalid)) / 1000; %converts to seconds

% flight summary
fprintf('Flight 2 Summary\n');
% print flight time
fprintf('Flight time: %.2f s\n', (max(tvalid) - min(tvalid)) / 1000);

n = size(signals, 1);
% loops through each row in the array
for i = 1:n
    % retrieves data for the current row in the array
    col   = signals{i, 1};
    label = signals{i, 2};
    unit  = signals{i, 3};
    limits = signals{i, 4};

    % skip only if the column header isn't in the CSV
    if ~ismember(col, T.Properties.VariableNames)
        warning('Column %s not found, skipping.', col);
        continue;
    end

    % retrieves the actual data for the row from the csv file
    y = T.(col);

    % checks that the data exists and is not physically impossible
    present = isfinite(t_ms) & isfinite(y); % not missing and not infinite
    corrupt = present & (y < limits(1) | y > limits(2));
    keep  = present & ~corrupt; % passed both checks

    % counts the total number of missing and corrupt
    missingSum = sum(~present);
    corruptSum = sum(corrupt);

    % stores cleaned samples for summary statistics and plotting
    yi = y(keep);

    % checks to see if the data is empty or not
    if isempty(yi)
        warning('%s has no valid samples, skipping.', label);
        continue;
    end

    % prints the summary for the current column
    fprintf('%s Max: %f  Min: %f  (missing %d, corrupt %d)\n', ...
            label, max(yi), min(yi), missingSum, corruptSum);

    % blank bad samples with NaN instead of deleting them,
    % so dropouts show as gaps instead of straight lines across them
    yplot = y;
    yplot(~keep) = NaN;

    % plotting the graph for current column
    figure;
    plot(t_s, yplot, 'b.-', 'LineWidth', 2);
    xlabel('Time (s)');
    ylabel(sprintf('%s (%s)', label, unit));
    title(sprintf('%s as a Function of Time', label));
    grid on;
end
%----------------------------
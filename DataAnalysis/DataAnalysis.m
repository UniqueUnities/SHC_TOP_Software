 % clears the command window
clearvars; close all; clc;

% checks if the file that will be parsed can be found or not
if ~isfile('datalog.csv')
    error('datalog.csv not found in the current folder.');
end

% force every column numeric at import. junk cells like "ERR" become
% NaN instead of turning the whole column into text
opts = detectImportOptions('datalog.csv');
opts = setvartype(opts, 'double');
T = readtable('datalog.csv', opts);

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

% possible error values from sensors
sentinels = [-999, -9999, 65535];

% {column name in CSV, display label, unit, min/max}
signals = {
    'Longitude','Longitude','deg', [-180, 180];
    'Latitude','Latitude','deg', [-90, 90];
    'Altitude','Altitude','m', [-Inf, Inf];
    'Pressure','Pressure','hPa', [-Inf, Inf];
    'Temperature','Temperature','°C', [-Inf, Inf];
    'Roll','Roll','deg', [-Inf, Inf];
    'Pitch','Pitch','deg', [-Inf, Inf];
    'Yaw','Yaw','deg', [-Inf Inf];
    'Ax','Acceleration (x)','m/s^2', [-Inf, Inf];
    'Ay','Acceleration (y)','m/s^2', [-Inf, Inf];
    'Az','Acceleration (z)','m/s^2', [-Inf, Inf];
    'Speed','Speed','km/h', [-Inf, Inf]
};

t_ms = T.Time; % retrieves data regarding time in milliseconds
tvalid = t_ms(isfinite(t_ms)); %retrieves valid data regarding time
t_s = (t_ms - min(tvalid)) / 1000; %converts to seconds

% flight summary
fprintf('Flight Summary\n');
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
    corrupt = present & (y < limits(1) | y > limits(2) | ismember(y, sentinels));
    if any(strcmp(col, {'Latitude', 'Longitude'}))
        corrupt = corrupt | (present & y == 0);
    end
    keep  = present & ~corrupt; % passed both checks

    % counts the total number of missing and corrupt
    nMissing = sum(~present);
    nCorrupt = sum(corrupt);

    % stores cleaned samples for summary statistics and plotting
    yi = y(keep);

    % checks to see if the data is empty or not
    if isempty(yi)
        warning('%s has no valid samples, skipping.', label);
        continue;
    end

    % prints the summary for the current column
    fprintf('%s Max: %f  Min: %f  (missing %d, corrupt %d)\n', ...
            label, max(yi), min(yi), nMissing, nCorrupt);

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
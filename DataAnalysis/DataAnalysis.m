 % clears the command window
clearvars; close all; clc;

% checks if the file that will be parsed can be found or not
if ~isfile('datalog.csv')
    error('datalog.csv not found in the current folder.');
end

% parses the csv file in the form of table
T = readtable('datalog.csv');

% makes sure every value in the column for time is a number
if ~isnumeric(T.Time)
    error('Time column is not numeric. Check the CSV for bad characters.');
end

% checks to make sure no time stamps stops or goes backwards
if any(diff(T.Time(~ismissing(T.Time))) <= 0)
    warning('Timestamps repeat or go backwards.');
end

% moving-median window for spike detection
outlierWindow = 25;

% {column name in CSV, display label, unit, min/max}
signals = {
    'Longitude','Longitude','deg', [-90, 90];
    'Latitude','Latitude','deg', [-90, 90];
    'Altitude','Altitude','m', [-Inf Inf];
    'Pressure','Pressure','hPa', [-Inf Inf];
    'Temperature','Temperature','°C', [-Inf Inf];
    'Roll','Roll','deg', [-Inf Inf];
    'Pitch','Pitch','deg', [-Inf Inf];
    'Yaw','Yaw','deg', [-Inf Inf];
    'Ax','Acceleration (x)','m/s^2', [-Inf Inf];
    'Ay','Acceleration (y)','m/s^2', [-Inf Inf];
    'Az','Acceleration (z)','m/s^2', [-Inf Inf];
    'Speed','Speed','km/h', [-Inf Inf]
};

t_ms = T.Time;                      % retrieves data regarding time in milliseconds
t_s  = (t_ms - min(t_ms)) / 1000;   % converts to seconds since start, easier to read

% flight summary
fprintf('Flight Summary\n');
fprintf('Flight time: %.2f s\n', (max(t_ms) - min(t_ms)) / 1000);

n = size(signals, 1);
% loops through each row in the array
for i = 1:n
    % retrieves data for the current row in the array
    col   = signals{i, 1};
    label = signals{i, 2};
    unit  = signals{i, 3};
    limits = signals{i, 4};

    % retrieves the actual data for the row from the csv file
    y = T.(col);

    % checks that the values in the column are all numbers
    if ~isnumeric(y)
        warning('%s imported as text, skipping.', label);
        continue;
    end

    % checks that the data exists and is not physically impossible
    present = ~ismissing(t_ms) & ~ismissing(y); % not missing
    corrupt = present & (y < limits(1) | y > limits(2)); % physically impossible
    usable  = present & ~corrupt; % passed both checks

    % outlier checking only on present samples
    % spans real data instead of NaN gaps
    keep = usable;
    keep(usable) = ~isoutlier(y(usable), 'movmedian', outlierWindow);

    % counts the total number of missing, corrupt, and spikes
    nMissing = sum(~present);
    nCorrupt = sum(corrupt);
    nSpikes  = sum(usable) - sum(keep);

    % stores cleaned samples for summary statistics and plotting
    ti = t_s(keep);
    yi = y(keep);


    if isempty(yi)
        warning('%s has no valid samples, skipping.', label);
        continue;
    end

    fprintf('%s Max: %f  Min: %f  (missing: %d, spikes:%d)\n', ...
            label, max(yi), min(yi), nMissing, nSpikes);

    % ---- Plot ----
    figure;
    plot(ti, yi, 'b-', 'LineWidth', 2);
    xlabel('Time (s)');
    ylabel(sprintf('%s (%s)', label, unit));
    title(sprintf('%s as a Function of Time', label));
    grid on;
end
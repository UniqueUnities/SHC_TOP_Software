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

% flight detection: time going backward means the Pico rebooted
idx = find(isfinite(t_ms)); % rows with a real timestamp
dt = diff(t_ms(idx)); % change between consecutive valid rows
resetAt = idx(find(dt < 0) + 1); % first row of each new flight

flightID = ones(height(T), 1); % tag every row with a flight number
for k = 1:numel(resetAt)
    flightID(resetAt(k):end) = flightID(resetAt(k):end) + 1;
end
nFlights = max(flightID);

% per-flight time axis in seconds, each flight starting at 0
t_s = nan(height(T), 1);
for f = 1:nFlights
    m = (flightID == f) & isfinite(t_ms);
    if any(m)
        t_s(m) = (t_ms(m) - min(t_ms(m))) / 1000;
    end
end

% flight summary
fprintf('Flight Summary\n');
fprintf('Flights detected: %d\n', nFlights);
for f = 1:nFlights
    m = (flightID == f) & isfinite(t_ms);
    fprintf('Flight %d time: %.2f s\n', f, (max(t_ms(m)) - min(t_ms(m))) / 1000);
end

colors = lines(nFlights);                % one color per flight

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

    % plotting: one figure per signal, one colored line per flight
    figure; hold on;
    for f = 1:nFlights
        inF = (flightID == f);
        yi_f = y(keep & inF);
        if isempty(yi_f)
            continue;
        end
        fprintf('  Flight %d  Max: %f  Min: %f\n', f, max(yi_f), min(yi_f));

        yp = yplot;
        yp(~inF) = NaN; % hide the other flight's points
        plot(t_s, yp, '.-', 'Color', colors(f,:), 'LineWidth', 2, ...
             'DisplayName', sprintf('Flight %d', f));
    end
    xlabel('Time (s)');
    ylabel(sprintf('%s (%s)', label, unit));
    title(sprintf('%s as a Function of Time', label));
    legend('show'); grid on;
end
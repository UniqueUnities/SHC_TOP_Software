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
    'Altitude','Altitude','ft', [490, 1000];
    'Pressure','Pressure','psi', [10, 50];
    'Temperature','Temperature','°F', [-10, 100];
    'Roll','Roll','deg', [-90, 90];
    'Pitch','Pitch','deg', [-60, 60];
    'Yaw','Yaw','deg', [0, 360];
    'Ax','Acceleration (x)','ft/s^2', [-120, 120];
    'Ay','Acceleration (y)','ft/s^2', [-120, 120];
    'Az','Acceleration (z)','ft/s^2', [-120, 120];
    'Speed','Speed','mph', [0, 80]
};

t_ms = T.Time; % time in milliseconds
tvalid = t_ms(isfinite(t_ms)); % valid time samples
t_s = (t_ms - min(tvalid)) / 1000; % convert to seconds

% altitude setup
launchThresh = 10; % ft above ground that counts as "in air"
alt = T.Altitude; % get altitude values
ok  = isfinite(t_s) & isfinite(alt) & alt >= 0 & alt <= 2000; % check numbers are valid
groundAlt = 519.6; % specific value for the RCRC
airborne  = ok & (alt > groundAlt + launchThresh);

% flight window from ground speed
spd = T.Speed;
spd(spd < 0 | spd > 100) = NaN;
moving = isfinite(spd) & spd > 8;

dm = diff([0; moving(:); 0]);
starts = find(dm ==  1);
ends   = find(dm == -1) - 1;
[~, k] = max(ends - starts); % longest contiguous run = the flight
iUp   = starts(k);
iDown = ends(k);

% time actually above the altitude threshold, inside the flight window
idx = (1:numel(alt))';
inWindow = idx >= iUp & idx <= iDown;
iAir = find(airborne & inWindow);

% flight summary
fprintf('Flight 1 Summary\n');
fprintf('Total time recorded: %.2f s\n', (max(tvalid) - min(tvalid)) / 1000);
fprintf('Flight time: %.2f s (start %.2f s, end %.2f s)\n', ...
    t_s(iDown) - t_s(iUp), t_s(iUp), t_s(iDown));

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
    plot(t_s, yplot, 'r.-', 'LineWidth', 2);
    xlabel('Time (s)');
    ylabel(sprintf('%s (%s)', label, unit));
    title(sprintf('Flight 1: %s as a Function of Time', label));
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
    'Altitude','Altitude','ft', [0, 2000];
    'Pressure','Pressure','psi', [0, 100];
    'Temperature','Temperature','°F', [-10, 100];
    'Roll','Roll','deg', [-180, 180];
    'Pitch','Pitch','deg', [-90, 90];
    'Yaw','Yaw','deg', [0, 360];
    'Ax','Acceleration (x)','ft/s^2', [-100, 100];
    'Ay','Acceleration (y)','ft/s^2', [-100, 100];
    'Az','Acceleration (z)','ft/s^2', [-100, 100];
    'Speed','Speed','mph', [0, 100]
};

t_ms = T.Time; % time in milliseconds
tvalid = t_ms(isfinite(t_ms)); % valid time samples
t_s = (t_ms - min(tvalid)) / 1000; % convert to seconds

% flight time from ground speed
spd = T.Speed; % get sped
spd(spd < 0 | spd > 100) = NaN; % check any corrupt/missing values for speed
moving = isfinite(spd) & spd > 8; % speed after than 8 mph is moving

% checking the range in time where the plane is moving
dm = diff([0; moving(:); 0]);
starts = find(dm ==  1);
ends   = find(dm == -1) - 1;
[~, k] = max(ends - starts); % longest contiguous run = the flight
iUp   = starts(k);
iDown = ends(k);

% flight summary
fprintf('Flight 1 Summary\n');
fprintf('Total time recorded: %.2f s\n', (max(tvalid) - min(tvalid)) / 1000);
fprintf('Flight time: %.2f s (start %.2f s, end %.2f s)\n', ...
    t_s(iDown) - t_s(iUp), t_s(iUp), t_s(iDown));

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
    title(sprintf('Flight 2: %s as a Function of Time', label));
    grid on;
end
%----------------------------
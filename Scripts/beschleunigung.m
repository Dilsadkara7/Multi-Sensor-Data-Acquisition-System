clear; clc; close all;
filePath = 'C:\Users\usr\hyperloop_test_data.csv';
data = readtable(filePath, 'VariableNamingRule', 'preserve');
t = (0:height(data)-1)' * 0.05; 
s = data.Mesafe_cm / 100; 

s_smooth = sgolayfilt(s, 3, 21); 

v = gradient(s_smooth, t); 
a = gradient(v, t);       

fig = figure('Color', 'k', 'Units', 'normalized', 'Position', [0.1 0.1 0.7 0.8]);
tlo = tiledlayout(3,1, 'Padding', 'tight', 'TileSpacing', 'compact');

nexttile;
plot(t, s, 'Color', [0.3 0.3 0.3], 'LineWidth', 0.5); hold on;
plot(t, s_smooth, 'c', 'LineWidth', 2);
ylabel('\bf Location [m]', 'Color', 'w');
grid on; set(gca, 'Color', 'k', 'XColor', 'w', 'YColor', 'w', 'GridColor', 'w');

nexttile;
plot(t, abs(v), 'r', 'LineWidth', 2.5);
ylabel('\bf Speed [m/s]', 'Color', 'w');
grid on; set(gca, 'Color', 'k', 'XColor', 'w', 'YColor', 'w', 'GridColor', 'w');

nexttile;
plot(t, a, 'g', 'LineWidth', 2.5);
ylabel('\bf Acceleration [m/s^2]', 'Color', 'w');
xlabel('\bf Time [s]', 'Color', 'w');
grid on; set(gca, 'Color', 'k', 'XColor', 'w', 'YColor', 'w', 'GridColor', 'w');

title(tlo, '\bf Kinematic Performance Summary', 'Color', 'w', 'FontSize', 16);
linkaxes(findall(fig, 'type', 'axes'), 'x');
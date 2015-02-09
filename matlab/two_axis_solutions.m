clear all
syms A_LED_x A_LED_y B_LED_x B_LED_y X_a X_b P_a P_b fil_x fil_y x y

%S = solve(((P_a - A_LED_y) / (0 - A_LED_x) * (x - A_LED_x)) + A_LED_y == ((0 - B_LED_y) / (P_b - B_LED_x) * (x - B_LED_x)) + B_LED_y, x)

%S = subs(S,P_a,X_a + A_LED_y);
%S = subs(S,P_b,X_b + B_LED_x)

[fil_x, fil_y] = solve(y == ((P_a - A_LED_y)/(0 - A_LED_x)*(x - A_LED_x)) + A_LED_y, ...
                       y == ((0 - B_LED_y)/(P_b - B_LED_x)*(x - B_LED_x)) + B_LED_y, ...
                       x, y)
                   
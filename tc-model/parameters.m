%run this model first

%motor
L = 177e-6; %inductance
R = 0.018; %resistance
Kb = 0.6970; %constant for motor size and back emf
Km = Kb;
J = 0.0421; %inertia
b = 3.97e-6; %viscous friction
fc = 0.0;

%car/traction parameters
m = 250; %mass of car
Rw = 0.1778; %wheel size
g = 9.81;
Q = 0.5; %weight balance factor
GR = 3.42; %gear ratio

%initial conditions
i_0 = 0;
wm_0 = 0;
tht_0 = 0;
v_0 = 0;
xp_0 = 0;

%tolerance
tol = 1.0e-10;

%desired speed to speed up to % change later
wm_des = 275;

% desered speed to slow down to 
wm_des2 = 110;

%input voltage
u1 = 537.6;





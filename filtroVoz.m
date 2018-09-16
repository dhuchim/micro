clc %limpiamos la ventana de comandos
clear all %eliminamos todas las variables
close all %cerramos todas las ventanas de graficos
Filename='salidaconfiltro.ogg'; %nombramos nuestro archivo de salida
[x,fs]=audioread('josejose.ogg'); %seleccionamos la cancion que filtraremos
%calculamos las frecuencias en terminos de pi por medio de una regla de 3
%para ello tomamos el valor fs que es la frecuencia para calcular estos
%limites
ws=1450*2*pi/fs; %en base a las frecuencias de la voz establecemos la banda de stop
wp=1550*2*pi/fs; % con el mismo criterio que la ws establecemos la banda de paso 
Rp = 0.5; As = 70; %indicamos las especificaciones
%para convertir las especificaciones relativas (dB) Rp y As en el
%especificaciones absolutas delta1 y delta2.
[delta1,delta2] = db2delta(Rp,As);
%se usa la instrucción para estimar el orden del filtro que satisfaga 
%las especificaciones
[N,f,m,weights] = firpmord([ws,wp]/pi,[0,1],[delta2,delta1]); 
%aplicamos el algoritmo de Parks-McClellan
h = firpm(N,f,m,weights);
%usamos la funcion freqz_m para obtener la respuesta en dB y en amplitud
[db,mag,pha,grd,w] = freqz_m(h,[1]);
%aplicamos el filtro
y=filter(h,1,x);
%graficamos la respuesta en amplitud
figure('Name','Respuestas','NumberTitle','off'),
subplot(2,1,1); plot(w/pi,mag);xlabel('Frecuencia en unidades de Pi');ylabel('Hr(w)');
title('Respuesta de amplitud'); axis([0 0.5 -0.1 1.2]);grid on
%graficamos la respuesta en Decibeles
subplot(2,1,2);plot(w/pi,db);xlabel('Frecuencia en unidades de Pi');ylabel('Decibeles');
title('Magnitud de respuesta en dB');axis([0 0.5 -100 20]);grid on
figure('Name','Comparacion de Sonidos','NumberTitle','off');
subplot(2,1,1);plot(x);title('Señal Original');
subplot(2,1,2);plot(y);title('Señal con filtro aplicado');
%sound(y,fs); %para escuchar el audio resultante
%audiowrite(Filename,y,fs)%guardamos el archivo resultante con el nombre de Filename







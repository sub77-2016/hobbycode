clear;
%str = num2str(t);
name = strcat('Corr','.txt');
% name = strcat(name,'.dat');
dat = load(name); 
[m,n] = size(dat);
Cor = dat(:,2:n);

R = zeros(m,1);
t = dat(:,1);

figure();
plot(Cor');
xlabel('r');
ylabel('C(r)');

for (i=1:m)
    r = 1;
    while( (Cor(i,r) >=0) && (r<n-1) ) 
        r = r + 1;
    end
    R(i) = r;        
end

% figure();
% subplot(211);
% plot(Cor');
% xlabel('r');
% ylabel('C(r)');
% 
% subplot(212);
% for (i=1:m)
%     Cr = Cor(i,:);
%     
%     N=2*(n-1);
%     
%     if mod(N,2)==0
%         k=-N/2:N/2-1; % N even
%     else
%         k=-(N-1)/2:(N-1)/2; % N odd
%     end
%  
%     freq=k/N; 
% 
%     X=fft(Cr,N); % normalize the data
%     X=fftshift(X);
%     Sk = real( X );
%     plot(freq, Sk); 
%     xlabel('k');
%     ylabel('S(k)');
%     hold on;
%     
%     % Compute R
%     %k = 1:n-1;
%     k_ = sum(freq.*Sk)/sum(Sk);
%     R(i) = 2*pi/k_;    
% end

Rth = (5)*t.^(1/3);

figure();
loglog(t,R,'--*',t,Rth,'-');
xlabel('t');
ylabel('R(t)');

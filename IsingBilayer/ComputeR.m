clear;
%str = num2str(t);
name = strcat('Corr','.dat');
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
    while(Cor(i,r)>=0) 
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
%     Sk = real( fftshift( fft(Cr) ) );
%     plot(Sk); 
%     xlabel('k');
%     ylabel('S(k)');
%     hold on;
%     
%     % Compute R
%     k = 1:n-1;
%     k_ = sum(k.*Sk)/sum(Sk);
%     R(i) = 1/k_;    
% end

figure();
loglog(t,R,'--*');
xlabel('t');
ylabel('R(t)');
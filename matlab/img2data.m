function [ residual, y ] = img2data (x)

gray = rgb2gray( x );
BW = edge( gray );

[height, width] = size(BW);
y = zeros(1, width);
residual = zeros(1, width);

% for i = 1:width
%     if max(BW(:, i)) > 0
%         val = 0;
%         count = 0;
%         for j = 1: height   
%             if BW(j,i) == 1
%                 val = val + j;
%                 count = count + 1;
%             end
%         end
%         BW(:,i) = zeros(height, 1);
%         val = round((val/count));
%         BW(val, i) = 1;
%     end
% end

imshowpair(gray,BW,'falsecolor')

SE = strel('line', 4, 0);
BWprime=imclose(BW, SE);
      
imshowpair(gray,BWprime,'falsecolor')

for i = 1:width
    for j = 0:height-1
        if BWprime(height-j,i) == 1
            y(1,i) = (j);
        end
    end
end

t = 1:width;
z = zeros(1, width);
c = fit (t', y', 'poly1');

coeffs = coeffvalues(c);

slope = coeffs(1);
intercept = coeffs(2);

for k = 1:width
    residual(1,k) = y(1,k) - ( slope * k + intercept );
end

plot (t, residual, t, z)
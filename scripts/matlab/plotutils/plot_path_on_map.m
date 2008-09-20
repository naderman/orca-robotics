function plot_path_on_map( localiseLogFile, ogMapFile, ogCellSize, ogOrigin )

truePath = load( localiseLogFile );

figure(1);
clf; hold on;
displayOgMap( ogMapFile, ogCellSize, ogOrigin );
plot(truePath(:,1),truePath(:,2),'b-');
axis equal

ogImage=imread(ogMapFile);
pathImage=ones(size(ogImage));
ogNumCells=[size(ogImage,2),size(ogImage,1)];

for i=1:size(truePath,1)
    coords=cellCoords(truePath(i,1:2),ogCellSize,ogOrigin,ogNumCells);
    pathImage(round(coords(2)),round(coords(1)),:) = [0 0 1];

    ogImage(round(coords(2)),round(coords(1)),:) = [0 0 1];
end

figure(2);
clf;
imshow(pathImage)

figure(3);
clf;
imshow(ogImage)

imwrite(pathImage,'pathImage.png','png');

function imageCoords=cellCoords( worldCoord, ogCellSize, ogOrigin, ogNumCells )

ogMapSize = ogNumCells .* ogCellSize;
coords = (worldCoord-ogOrigin)./ogMapSize.*ogNumCells;
imageCoords(1) = coords(1);
imageCoords(2) = ogNumCells(2)-coords(2);
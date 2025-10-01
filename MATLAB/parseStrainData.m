clear all

baseFolderLoc = 'C:\Users\Clarissa G\Documents\MATLAB\Running Drosophibot\Trials\';
trochAxFileName = '08-Nov-2023 17_47_55';
trochTrFileName = '08-Nov-2023 17_45_13';
femRedFileName = '08-Nov-2023 17_20_12';
femBlueFileName = '08-Nov-2023 17_17_36';

trialNames = {trochAxFileName,trochTrFileName,femRedFileName,femBlueFileName};

for tr=1:4
    trialData = load([baseFolderLoc trialNames{tr} '\robotData.mat']);
    strain{tr} = trialData.strainDataRawNums;
    strain{tr} = strain{tr}-strain{tr}(1,:);
    strain{tr} = movmedian(strain{tr},7);

    timeVec{tr} = trialData.timeVecStrain;
end

figure
tl = tiledlayout(2,1,'TileSpacing','tight','Padding','tight');
nexttile
plot(timeVec{1},strain{1}(:,3),'b','LineWidth',0.75)
hold on
plot(timeVec{2},strain{2}(:,3),'r','LineWidth',0.75)
axis([0 timeVec{1}(end) -600 200])
grid on
xlabel('Time (s)')
ylabel('Microstrain (\mu\epsilon)')
title('Trochanteral Field Strain')
legend('Axial','Transverse','Location','southwest')

nexttile
plot(timeVec{3},strain{3}(:,3),'b','LineWidth',0.75)
hold on
plot(timeVec{4},strain{4}(:,3),'r','LineWidth',0.75)
xlim([0 timeVec{3}(end)])
grid on
xlabel('Time (s)')
ylabel('Microstrain (\mu\epsilon)')
title('Femoral Field Strain')
legend('Pos 45 ^{\circ}','Neg 45 ^{\circ}','Location','southwest')
title(tl,'LM Walking Strains')
subtitle(tl,'T_{step}= 2s')
set(findall(gcf,'-property','FontSize'),'FontSize',14)

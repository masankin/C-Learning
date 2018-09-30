
--------------------------------
-- @module TableManager
-- @extend Ref
-- @parent_module 

--------------------------------
-- 
-- @function [parent=#TableManager] getCreepsPoint 
-- @param self
-- @param #int id
-- @return CreepsPoint#CreepsPoint ret (return value: CreepsPoint)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getBadgeDataTable 
-- @param self
-- @return BadgeDataTable#BadgeDataTable ret (return value: BadgeDataTable)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getAchievementAwardCount 
-- @param self
-- @param #int 
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getNextCheckPoints 
-- @param self
-- @param #int cpCur
-- @param #int cpStar
-- @return array_table#array_table ret (return value: array_table)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getPowerBuy 
-- @param self
-- @param #int id
-- @return PowerBuy#PowerBuy ret (return value: PowerBuy)
        
--------------------------------
-- 
-- @function [parent=#TableManager] initLoadTables 
-- @param self
-- @return TableManager#TableManager self (return value: TableManager)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getShopInfoById 
-- @param self
-- @param #int id
-- @return ShopInfo#ShopInfo ret (return value: ShopInfo)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getJungleDataByID 
-- @param self
-- @param #int buildID
-- @return JungleData#JungleData ret (return value: JungleData)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getItemBuyById 
-- @param self
-- @param #int id
-- @return ItemBuyInfo#ItemBuyInfo ret (return value: ItemBuyInfo)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getPlatformName 
-- @param self
-- @param #int id
-- @return string#string ret (return value: string)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getNoticeData 
-- @param self
-- @param #int id
-- @return NoticeData#NoticeData ret (return value: NoticeData)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getWinnerDataNum 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getGoodsInfo 
-- @param self
-- @param #int type
-- @param #int id
-- @return GoodsInfo#GoodsInfo ret (return value: GoodsInfo)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getBible 
-- @param self
-- @param #int id
-- @return Bible#Bible ret (return value: Bible)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getNameArea 
-- @param self
-- @param #int id
-- @return NameArea#NameArea ret (return value: NameArea)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getMusicInfoByID 
-- @param self
-- @param #int 
-- @return MusicInfo#MusicInfo ret (return value: MusicInfo)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getDayTaskDataById 
-- @param self
-- @param #int id
-- @return DayTaskData#DayTaskData ret (return value: DayTaskData)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getCreeps 
-- @param self
-- @param #int id
-- @return Creeps#Creeps ret (return value: Creeps)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getPayLimit 
-- @param self
-- @return PayLimit#PayLimit ret (return value: PayLimit)
        
--------------------------------
-- 
-- @function [parent=#TableManager] loadNoticeTable 
-- @param self
-- @return TableManager#TableManager self (return value: TableManager)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getActLogin 
-- @param self
-- @param #int day
-- @return ActLogin#ActLogin ret (return value: ActLogin)
        
--------------------------------
-- 
-- @function [parent=#TableManager] loadShopTable 
-- @param self
-- @return TableManager#TableManager self (return value: TableManager)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getPropType 
-- @param self
-- @param #int id
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getNewPlayGiftDataByType 
-- @param self
-- @param #int type
-- @return NewPlayGiftData#NewPlayGiftData ret (return value: NewPlayGiftData)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getPvpChat 
-- @param self
-- @param #int id
-- @return PvpChat#PvpChat ret (return value: PvpChat)
        
--------------------------------
-- 
-- @function [parent=#TableManager] unloadAllTables 
-- @param self
-- @return TableManager#TableManager self (return value: TableManager)
        
--------------------------------
-- 
-- @function [parent=#TableManager] loadAllTables 
-- @param self
-- @return TableManager#TableManager self (return value: TableManager)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getBuffDataByID 
-- @param self
-- @param #int buffID
-- @return BuffData#BuffData ret (return value: BuffData)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getMapCPoint 
-- @param self
-- @param #int type
-- @param #int mapid
-- @return CheckPoint#CheckPoint ret (return value: CheckPoint)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getExchangeItemCount 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getExchangeItem 
-- @param self
-- @param #int id
-- @return ExchangeItem#ExchangeItem ret (return value: ExchangeItem)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getBuildIns 
-- @param self
-- @param #int 
-- @return BuildIns#BuildIns ret (return value: BuildIns)
        
--------------------------------
-- 
-- @function [parent=#TableManager] isAchieveMentAwardComplent 
-- @param self
-- @param #int 
-- @param #bool isLast
-- @return bool#bool ret (return value: bool)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getPropIdByType 
-- @param self
-- @param #int type
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getBadgeData 
-- @param self
-- @param #int id
-- @return BadgeData#BadgeData ret (return value: BadgeData)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getGuideDataByID 
-- @param self
-- @param #int guideID
-- @return GuideData#GuideData ret (return value: GuideData)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getGradeData 
-- @param self
-- @param #int point
-- @return GradeData#GradeData ret (return value: GradeData)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getActInformDataByIdx 
-- @param self
-- @param #int idx
-- @return ActInformData#ActInformData ret (return value: ActInformData)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getGradeByGrade 
-- @param self
-- @param #int gradeId
-- @return GradeData#GradeData ret (return value: GradeData)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getGiftAward 
-- @param self
-- @param #int id
-- @return GiftAward#GiftAward ret (return value: GiftAward)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getAchievementById 
-- @param self
-- @param #int id
-- @return AchievementAward#AchievementAward ret (return value: AchievementAward)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getAnimationNameByExpress 
-- @param self
-- @param #int type
-- @return string#string ret (return value: string)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getWinnerData 
-- @param self
-- @param #int id
-- @return WinnerData#WinnerData ret (return value: WinnerData)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getAnimationNameByCamp 
-- @param self
-- @param #int type
-- @return string#string ret (return value: string)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getRechargeData 
-- @param self
-- @param #int id
-- @return RechargeData#RechargeData ret (return value: RechargeData)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getActInformData 
-- @param self
-- @param #int id
-- @return ActInformData#ActInformData ret (return value: ActInformData)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getLoadState 
-- @param self
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getAchievePointAwardById 
-- @param self
-- @param #int id
-- @return AchievePointAward#AchievePointAward ret (return value: AchievePointAward)
        
--------------------------------
-- 
-- @function [parent=#TableManager] setLoadState 
-- @param self
-- @param #int state
-- @return TableManager#TableManager self (return value: TableManager)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getBuildAttr 
-- @param self
-- @param #int id
-- @return BuildAttr#BuildAttr ret (return value: BuildAttr)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getSevenDayMissionByDay 
-- @param self
-- @param #int day
-- @return SevenDayMission#SevenDayMission ret (return value: SevenDayMission)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getErrorCodeByID 
-- @param self
-- @param #int id
-- @return ErrorCode#ErrorCode ret (return value: ErrorCode)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getInsistuteAddAttr 
-- @param self
-- @param #int id
-- @return InsistuteAddAttr#InsistuteAddAttr ret (return value: InsistuteAddAttr)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getMoraleLevel 
-- @param self
-- @param #float moraleValue
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#TableManager] loadPlatformCsv 
-- @param self
-- @return TableManager#TableManager self (return value: TableManager)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getPlayerInfo 
-- @param self
-- @return PlayerInfo#PlayerInfo ret (return value: PlayerInfo)
        
--------------------------------
-- 
-- @function [parent=#TableManager] loadActInformTable 
-- @param self
-- @return TableManager#TableManager self (return value: TableManager)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getCrazyWeekInfoById 
-- @param self
-- @param #int id
-- @return CrazyWeekInfo#CrazyWeekInfo ret (return value: CrazyWeekInfo)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getPropInfoById 
-- @param self
-- @param #int id
-- @return PropInfo#PropInfo ret (return value: PropInfo)
        
--------------------------------
-- 
-- @function [parent=#TableManager] update 
-- @param self
-- @param #float delta
-- @return TableManager#TableManager self (return value: TableManager)
        
--------------------------------
-- 
-- @function [parent=#TableManager] loadGuideTable 
-- @param self
-- @return TableManager#TableManager self (return value: TableManager)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getFlagNameByCamp 
-- @param self
-- @param #int type
-- @return string#string ret (return value: string)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getTextHelperByID 
-- @param self
-- @param #int id
-- @return TextHelper#TextHelper ret (return value: TextHelper)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getGlyphStuffData 
-- @param self
-- @param #int id
-- @return GlyphStuffData#GlyphStuffData ret (return value: GlyphStuffData)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getMoraleDefenceDataByRange 
-- @param self
-- @param #float moraleValue
-- @return MoraleDefenceData#MoraleDefenceData ret (return value: MoraleDefenceData)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getWolrdAllStar 
-- @param self
-- @param #int wolrd
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getLevelExp 
-- @param self
-- @param #int level
-- @return int#int ret (return value: int)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getScoreInfo 
-- @param self
-- @param #int id
-- @return ScoreInfo#ScoreInfo ret (return value: ScoreInfo)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getStateActionByID 
-- @param self
-- @param #int 
-- @return StateAction#StateAction ret (return value: StateAction)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getIntelligent 
-- @param self
-- @param #int id
-- @return Intelligent#Intelligent ret (return value: Intelligent)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getVipData 
-- @param self
-- @param #int id
-- @return VipData#VipData ret (return value: VipData)
        
--------------------------------
-- 
-- @function [parent=#TableManager] loadErrorCodeCsv 
-- @param self
-- @return TableManager#TableManager self (return value: TableManager)
        
--------------------------------
-- 
-- @function [parent=#TableManager] getAchievementAwardByID 
-- @param self
-- @param #int 
-- @param #int 
-- @return AchievementAward#AchievementAward ret (return value: AchievementAward)
        
--------------------------------
-- 
-- @function [parent=#TableManager] TableManager 
-- @param self
-- @return TableManager#TableManager self (return value: TableManager)
        
return nil

--RUM－千死蛮巧
function c96142517.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCost(c96142517.cost)
	e1:SetTarget(c96142517.target)
	e1:SetOperation(c96142517.activate)
	c:RegisterEffect(e1)
end
function c96142517.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetActivityCount(tp,ACTIVITY_SPSUMMON)==0 end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_OATH)
	e1:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e1:SetReset(RESET_PHASE+PHASE_END)
	e1:SetTargetRange(1,0)
	e1:SetTarget(c96142517.sumlimit)
	Duel.RegisterEffect(e1,tp)
end
function c96142517.sumlimit(e,c,sump,sumtype,sumpos,targetp,se)
	return e:GetHandler()~=se:GetHandler()
end
function c96142517.filter1(c,e)
	return c:IsType(TYPE_XYZ) and c:IsCanBeEffectTarget(e)
end
function c96142517.filter2(c,e,tp)
	return (c:IsSetCard(0x1048) or c:IsSetCard(0x1073)) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c96142517.spfilter(c,e,tp,rk,mg)
	if c:GetOriginalCode()==6165656 and mg:FilterCount(Card.IsCode,nil,48995978)==0 then return false end
	return c:IsRank(rk) and (c:IsSetCard(0x1048) or c:IsSetCard(0x1073)) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c96142517.targetcheck1(c,g2,g3)
	local rk=c:GetRank()
	return g2:IsExists(Card.IsRank,1,nil,rk) and g3:IsExists(Card.IsRank,1,nil,rk+1)
end
function c96142517.targetcheck2(c)
	return c:GetOriginalCode()~=6165656
end
function c96142517.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	local g1=Duel.GetMatchingGroup(c96142517.filter1,tp,LOCATION_GRAVE,0,nil,e) --avaliable targets in your graveyard
	local g2=Duel.GetMatchingGroup(c96142517.filter1,tp,0,LOCATION_GRAVE,nil,e) --avaliable targets in opponent's graveyard
	local g3=Duel.GetMatchingGroup(c96142517.filter2,tp,LOCATION_EXTRA,0,nil,e,tp) --avaliable group to special summon
	local tg=g1:Filter(c96142517.targetcheck1,nil,g2,g3)
	--a group of avaliable targets that has at least 1 same-ranked target in opponent's graveyard. 
	local v1,v2=g3:FilterCount(c96142517.targetcheck2,nil)==0,
		g1:FilterCount(Card.IsCode,nil,48995978)+g2:FilterCount(Card.IsCode,nil,48995978)==0
	--if "v1" is true, means you have nothing to special summon but C88.
	--if "v2" is true, means there are no 88 to target in either's graveyard. 
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and tg:GetCount()~=0 and not (v1 and v2) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local tg1=tg:Select(tp,1,1,nil)
	Duel.HintSelection(tg1)
	local rank=tg1:GetFirst():GetRank()
	e:SetLabel(rank)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local tg2=g2:FilterSelect(tp,Card.IsRank,1,1,nil,rank)
	Duel.HintSelection(tg2)
	g1:Sub(tg1) g2:Sub(tg2) g1:Merge(g2)
	if v1 and tg1:FilterCount(Card.IsCode,nil,48995978)+tg2:FilterCount(Card.IsCode,nil,48995978)==0 then
		Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(96142517,0))
		local tg4=g1:Filter(Card.IsRank,nil,rank):FilterSelect(tp,Card.IsCode,1,1,nil,48995978)
		tg1:Merge(tg4) g1:Sub(tg4)
	end
	if g1:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(96142517,1)) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
		local tg3=g1:FilterSelect(tp,Card.IsRank,1,99,nil,rank)
		Duel.HintSelection(tg3)
	end
	tg1:Merge(tg2) tg1:Merge(tg3)
	Duel.SetTargetCard(tg1)
	Duel.SetOperationInfo(0,CATEGORY_LEAVE_GRAVE,g1,g1:GetCount(),0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c96142517.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local rk=e:GetLabel()
	local mg=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local rg=mg:Filter(Card.IsRelateToEffect,nil,e)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c96142517.spfilter,tp,LOCATION_EXTRA,0,1,1,nil,e,tp,rk+1,mg)
	local sc=g:GetFirst()
	if sc and Duel.SpecialSummon(sc,0,tp,tp,false,false,POS_FACEUP)~=0 then
		Duel.Overlay(sc,rg)
	end
end

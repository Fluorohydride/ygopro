--ＲＵＭ－千死蛮巧
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
	if chk==0 then return not Duel.CheckSpecialSummonActivity(tp) end
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
function c96142517.filter1(c,e,tp)
	return c:IsFaceup() and c:IsType(TYPE_XYZ)
	and Duel.IsExistingMatchingCard(c96142517.filter2,tp,LOCATION_EXTRA,0,1,nil,e,tp,c:GetRank()+1)
	and Duel.IsExistingMatchingCard(c96142517.filter3,tp,0,LOCATION_GRAVE,1,nil,c:GetRank())
end
function c96142517.filter2(c,e,tp,rank)
	return c:GetRank()==rank and (c:IsSetCard(0x1048) or c:IsSetCard(0x1073))
		and c:IsCanBeSpecialSummoned(e,SUMMON_TYPE_XYZ,tp,false,false)
end
function c96142517.filter3(c,rank)
	return c:GetRank()==rank and c:IsType(TYPE_XYZ)
end
function c96142517.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and c96142517.filter1(chkc,e,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingTarget(c96142517.filter1,tp,LOCATION_GRAVE,0,1,nil,e,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local tg=Duel.SelectTarget(tp,c96142517.filter1,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	local rank=tg:GetFirst():GetRank()
	e:SetLabel(rank)
	local tg2=Duel.SelectTarget(tp,c96142517.filter3,tp,0,LOCATION_GRAVE,1,1,nil,rank)
	tg:Merge(tg2)
	local g=Duel.GetMatchingGroup(c96142517.filter3,tp,LOCATION_GRAVE,LOCATION_GRAVE,tg:GetFirst(),rank)
	g:Sub(tg2)
	if g:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(96142517,1)) then
		g=g:Select(tp,1,99,nil)
		tg:Merge(g)
		local tc=g:GetFirst()
		while tc do
			Duel.SetTargetCard(tc)
			tc=g:GetNext()
		end
	end
	Duel.SetOperationInfo(0,CATEGORY_LEAVE_GRAVE,tg,2,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_EXTRA)
end
function c96142517.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<0 then return end
	local rank=e:GetLabel()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c96142517.filter2,tp,LOCATION_EXTRA,0,1,1,nil,e,tp,rank+1)
	local sc=g:GetFirst()
	if sc then
		local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
		Duel.Overlay(sc,g)
		Duel.SpecialSummon(sc,SUMMON_TYPE_XYZ,tp,tp,false,false,POS_FACEUP)
		sc:CompleteProcedure()
	end
end

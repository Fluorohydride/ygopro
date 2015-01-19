--復活の墓穴
function c84136000.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c84136000.condition)
	e1:SetTarget(c84136000.target)
	e1:SetOperation(c84136000.activate)
	c:RegisterEffect(e1)
end
function c84136000.cfilter(c,tp)
	return c:IsLocation(LOCATION_GRAVE) and c:GetPreviousControler()==tp and c:IsReason(REASON_BATTLE)
end
function c84136000.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c84136000.cfilter,1,nil,tp)
end
function c84136000.spfilter(c,e,tp)
	return c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c84136000.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.GetLocationCount(1-tp,LOCATION_MZONE)>0
		and Duel.IsExistingTarget(c84136000.spfilter,tp,LOCATION_GRAVE,0,1,nil,e,tp)
		and Duel.IsExistingTarget(c84136000.spfilter,1-tp,LOCATION_GRAVE,0,1,nil,e,1-tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g1=Duel.SelectTarget(tp,c84136000.spfilter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_SPSUMMON)
	local g2=Duel.SelectTarget(1-tp,c84136000.spfilter,1-tp,LOCATION_GRAVE,0,1,1,nil,e,1-tp)
	g1:Merge(g2)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g1,2,PLAYER_ALL,g1:GetFirst():GetOwner())
end
function c84136000.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local tc=g:GetFirst()
	while tc do
		if tc:IsRelateToEffect(e) and Duel.SpecialSummonStep(tc,0,tc:GetControler(),tc:GetControler(),false,false,POS_FACEUP_DEFENCE) then
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_CANNOT_CHANGE_POSITION)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			tc:RegisterEffect(e1,true)
		end
		tc=g:GetNext()
	end
	Duel.SpecialSummonComplete()
end

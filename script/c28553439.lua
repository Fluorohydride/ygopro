--ディメンション・マジック
function c28553439.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1c0)
	e1:SetCondition(c28553439.condition)
	e1:SetTarget(c28553439.target)
	e1:SetOperation(c28553439.activate)
	c:RegisterEffect(e1)
end
function c28553439.cfilter(c)
	return c:IsFaceup() and c:IsRace(RACE_SPELLCASTER)
end
function c28553439.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c28553439.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c28553439.filter(c,e,tp)
	return c:IsRace(RACE_SPELLCASTER) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c28553439.rfilter(c,e)
	return c:IsReleasableByEffect() and c:IsCanBeEffectTarget(e)
end
function c28553439.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c28553439.rfilter(chkc,e) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.CheckReleaseGroup(tp,c28553439.rfilter,1,nil,e)
		and Duel.IsExistingMatchingCard(c28553439.filter,tp,LOCATION_HAND,0,1,nil,e,tp) end
	local g=Duel.SelectReleaseGroup(tp,c28553439.rfilter,1,1,nil,e)
	Duel.SetTargetCard(g)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_HAND)
end
function c28553439.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and not tc:IsImmuneToEffect(e) then
		if Duel.Release(tc,REASON_EFFECT)==0 then return end
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local sg=Duel.SelectMatchingCard(tp,c28553439.filter,tp,LOCATION_HAND,0,1,1,nil,e,tp)
		if sg:GetCount()==0 then return end
		Duel.BreakEffect()
		Duel.SpecialSummon(sg,0,tp,tp,false,false,POS_FACEUP)
		local dg=Duel.GetMatchingGroup(Card.IsDestructable,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
		if dg:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(28553439,0)) then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
			local des=dg:Select(tp,1,1,nil)
			Duel.HintSelection(des)
			Duel.BreakEffect()
			Duel.Destroy(des,REASON_EFFECT)
		end
	end
end

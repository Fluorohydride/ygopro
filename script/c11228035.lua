--エクシーズ・ソウル
function c11228035.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_DAMAGE_STEP)
	e1:SetCondition(c11228035.condition)
	e1:SetTarget(c11228035.target)
	e1:SetOperation(c11228035.activate)
	c:RegisterEffect(e1)
end
function c11228035.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()~=PHASE_DAMAGE or not Duel.IsDamageCalculated()
end
function c11228035.filter(c)
	return c:IsType(TYPE_XYZ)
end
function c11228035.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and c11228035.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c11228035.filter,tp,LOCATION_GRAVE,LOCATION_GRAVE,1,nil)
		and Duel.IsExistingMatchingCard(Card.IsFaceup,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	Duel.SelectTarget(tp,c11228035.filter,tp,LOCATION_GRAVE,LOCATION_GRAVE,1,1,nil)
end
function c11228035.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local g=Duel.GetMatchingGroup(Card.IsFaceup,tp,LOCATION_MZONE,0,nil)
	if g:GetCount()>0 and tc:IsRelateToEffect(e) then
		local atk=tc:GetRank()*200
		local sc=g:GetFirst()
		while sc do
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_UPDATE_ATTACK)
			e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_END)
			e1:SetValue(atk)
			sc:RegisterEffect(e1)
			sc=g:GetNext()
		end
		if tc:IsAbleToDeck() and not tc:IsHasEffect(EFFECT_NECRO_VALLEY)
			and Duel.SelectYesNo(tp,aux.Stringid(11228035,0)) then
			Duel.BreakEffect()
			Duel.SendtoDeck(tc,nil,2,REASON_EFFECT)
		end
	end
end

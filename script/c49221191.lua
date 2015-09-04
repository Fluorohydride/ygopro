--CNo.32 海咬龍シャーク・ドレイク・バイス
function c49221191.initial_effect(c)
	--xyz summon
	c:EnableReviveLimit()
	aux.AddXyzProcedure(c,aux.FilterBoolFunction(Card.IsAttribute,ATTRIBUTE_WATER),4,4,c49221191.ovfilter,aux.Stringid(49221191,0))
	--atk/def
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(49221191,1))
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetType(EFFECT_TYPE_QUICK_O)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetRange(LOCATION_MZONE)
	e1:SetHintTiming(TIMING_DAMAGE_STEP)
	e1:SetCondition(c49221191.condition)
	e1:SetCost(c49221191.cost)
	e1:SetTarget(c49221191.target)
	e1:SetOperation(c49221191.operation)
	c:RegisterEffect(e1)
end
c49221191.xyz_number=32
function c49221191.ovfilter(c)
	return c:IsFaceup() and c:IsCode(65676461)
end
function c49221191.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetLP(tp)<=1000 and (Duel.GetCurrentPhase()~=PHASE_DAMAGE or not Duel.IsDamageCalculated())
end
function c49221191.rfilter(c)
	return c:IsType(TYPE_MONSTER) and c:IsAbleToRemoveAsCost()
end
function c49221191.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST)
		and Duel.IsExistingMatchingCard(c49221191.rfilter,tp,LOCATION_GRAVE,0,1,nil) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c49221191.rfilter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.Remove(g,POS_FACEUP,REASON_COST)
end
function c49221191.filter(c)
	return c:IsFaceup() and (c:GetAttack()>0 or c:GetDefence()>0)
end
function c49221191.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c49221191.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c49221191.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c49221191.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
end
function c49221191.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetCode(EFFECT_SET_ATTACK_FINAL)
		e1:SetValue(0)
		if Duel.GetTurnPlayer()~=tp then
			e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,2)
		else
			e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,1)
		end
		tc:RegisterEffect(e1)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_SET_DEFENCE_FINAL)
		tc:RegisterEffect(e2)
	end
end

--虚構王アンフォームド・ボイド
function c38180759.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,4,3)
	c:EnableReviveLimit()
	--adup
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(38180759,0))
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_QUICK_O)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1c0+TIMING_MAIN_END)
	e1:SetCountLimit(1)
	e1:SetCondition(c38180759.atkcon)
	e1:SetCost(c38180759.atkcost)
	e1:SetTarget(c38180759.atktg)
	e1:SetOperation(c38180759.atkop)
	c:RegisterEffect(e1)
end
function c38180759.atkcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp and (Duel.GetCurrentPhase()==PHASE_MAIN1 or Duel.GetCurrentPhase()==PHASE_MAIN2)
end
function c38180759.atkcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c38180759.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_XYZ)
end
function c38180759.atktg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c38180759.filter,tp,0,LOCATION_MZONE,1,nil) end
end
function c38180759.atkop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsFaceup() and c:IsRelateToEffect(e) then
		local g=Duel.GetMatchingGroup(c38180759.filter,tp,0,LOCATION_MZONE,nil)
		local atk=g:GetSum(Card.GetAttack)
		if atk>0 then
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_UPDATE_ATTACK)
			e1:SetValue(atk)
			e1:SetReset(RESET_EVENT+0x1ff0000)
			c:RegisterEffect(e1)
			local e2=e1:Clone()
			e2:SetCode(EFFECT_UPDATE_DEFENCE)
			c:RegisterEffect(e2)
		end
	end
end

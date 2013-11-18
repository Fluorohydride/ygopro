--強欲な壺の精霊
function c4896788.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_CHAIN_SOLVING)
	e1:SetCondition(c4896788.drcon)
	e1:SetOperation(c4896788.drop)
	c:RegisterEffect(e1)
end
function c4896788.drcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsAttackPos()
		and re:IsHasType(EFFECT_TYPE_ACTIVATE) and re:GetHandler():IsCode(55144522)
end
function c4896788.drop(e,tp,eg,ep,ev,re,r,rp)
	Duel.ChangeTargetParam(ev,3)
end

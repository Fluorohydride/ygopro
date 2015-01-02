--フォトン・サークラー
function c64145892.initial_effect(c)
	--damage reduce
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_SINGLE)
	e1:SetCode(EVENT_PRE_BATTLE_DAMAGE)
	e1:SetCondition(c64145892.rdcon)
	e1:SetOperation(c64145892.rdop)
	c:RegisterEffect(e1)
end
function c64145892.rdcon(e,tp,eg,ep,ev,re,r,rp)
	return ep==tp
end
function c64145892.rdop(e,tp,eg,ep,ev,re,r,rp)
	Duel.ChangeBattleDamage(ep,ev/2)
end

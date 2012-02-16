--クレボンス
function c59575539.initial_effect(c)
	--be target
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(59575539,0))
	e1:SetCode(EVENT_BE_BATTLE_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCost(c59575539.cost)
	e1:SetOperation(c59575539.operation)
	c:RegisterEffect(e1)
end
function c59575539.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,800) end
	Duel.PayLPCost(tp,800)
end
function c59575539.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateAttack()
end

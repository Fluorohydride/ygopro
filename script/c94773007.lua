--地雷蜘蛛
function c94773007.initial_effect(c)
	--attack cost
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_ATTACK_COST)
	e1:SetCost(aux.TRUE)
	e1:SetOperation(c94773007.atop)
	c:RegisterEffect(e1)
end
function c94773007.atop(e,tp,eg,ep,ev,re,r,rp)
	local opt=Duel.SelectOption(tp,60,61)
	local coin=Duel.TossCoin(tp,1)
	if opt==coin then
		Duel.SetLP(tp,Duel.GetLP(tp)/2)
	end
end

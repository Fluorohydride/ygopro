--ダーク・エルフ
function c21417692.initial_effect(c)
	--attack cost
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_ATTACK_COST)
	e1:SetCost(c21417692.atcost)
	e1:SetOperation(c21417692.atop)
	c:RegisterEffect(e1)
end
function c21417692.atcost(e,c,tp)
	return Duel.CheckLPCost(tp,1000)
end
function c21417692.atop(e,tp,eg,ep,ev,re,r,rp)
	Duel.PayLPCost(tp,1000)
end

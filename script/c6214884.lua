--暗黒界の狂王 ブロン
function c6214884.initial_effect(c)
	--discard
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(6214884,0))
	e1:SetCategory(CATEGORY_HANDES)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_BATTLE_DAMAGE)
	e1:SetCondition(c6214884.condition)
	e1:SetTarget(c6214884.target)
	e1:SetOperation(c6214884.operation)
	c:RegisterEffect(e1)
end
function c6214884.condition(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp
end
function c6214884.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)>0 end
	Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,tp,1)
end
function c6214884.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.DiscardHand(tp,aux.TRUE,1,1,REASON_EFFECT+REASON_DISCARD)
end

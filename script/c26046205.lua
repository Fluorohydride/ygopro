--悪シノビ
function c26046205.initial_effect(c)
	--draw
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(26046205,0))
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetCode(EVENT_BE_BATTLE_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCondition(c26046205.condition)
	e1:SetTarget(c26046205.target)
	e1:SetOperation(c26046205.operation)
	c:RegisterEffect(e1)
end
function c26046205.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsPosition(POS_FACEUP_ATTACK)
end
function c26046205.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c26046205.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Draw(tp,1,REASON_EFFECT)
end

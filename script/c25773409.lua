--伝説の柔術家
function c25773409.initial_effect(c)
	--to deck
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(25773409,0))
	e1:SetCategory(CATEGORY_TODECK)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_DAMAGE_STEP_END)
	e1:SetCondition(c25773409.condition)
	e1:SetTarget(c25773409.target)
	e1:SetOperation(c25773409.operation)
	c:RegisterEffect(e1)
end
function c25773409.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetAttackTarget()==e:GetHandler() and bit.band(e:GetHandler():GetBattlePosition(),POS_DEFENCE)~=0
end
function c25773409.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_TODECK,Duel.GetAttacker(),1,0,0)
end
function c25773409.operation(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	if not a:IsRelateToBattle() then return end
	Duel.SendtoDeck(a,nil,0,REASON_EFFECT)
end

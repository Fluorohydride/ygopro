--ゴロゴル
function c59797187.initial_effect(c)
	--pos
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(59797187,0))
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_DAMAGE_STEP_END)
	e1:SetTarget(c59797187.target)
	e1:SetOperation(c59797187.operation)
	c:RegisterEffect(e1)
end
function c59797187.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local tg=e:GetHandler():GetBattleTarget()
	if chk==0 then return tg and tg:IsRelateToBattle() end
	Duel.SetOperationInfo(0,CATEGORY_POSITION,tg,1,0,0)
end
function c59797187.operation(e,tp,eg,ep,ev,re,r,rp)
	local bc=e:GetHandler():GetBattleTarget()
	if bc:IsRelateToBattle() and bc:IsFaceup() then
		Duel.ChangePosition(bc,POS_FACEDOWN_DEFENCE)
	end
end

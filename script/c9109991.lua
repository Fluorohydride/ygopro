--BF－鉄鎖のフェーン
function c9109991.initial_effect(c)
	--direct attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DIRECT_ATTACK)
	c:RegisterEffect(e1)
	--change pos
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(9109991,0))
	e2:SetCategory(CATEGORY_POSITION)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EVENT_DAMAGE)
	e2:SetCondition(c9109991.condition)
	e2:SetTarget(c9109991.target)
	e2:SetOperation(c9109991.operation)
	c:RegisterEffect(e2)
end
function c9109991.condition(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp and r==REASON_BATTLE and Duel.GetAttacker()==e:GetHandler() and Duel.GetAttackTarget()==nil
end
function c9109991.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:GetControler()~=tp and chkc:IsLocation(LOCATION_MZONE) and chkc:IsAttack() end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATTACK)
	local g=Duel.SelectTarget(tp,Card.IsAttack,tp,0,LOCATION_MZONE,1,1,nil)
	if g:GetCount()>0 then
		Duel.SetOperationInfo(0,CATEGORY_POSITION,g,1,0,0)
	end
end
function c9109991.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) and tc:IsAttack() then
		Duel.ChangePosition(tc,POS_FACEUP_DEFENCE,POS_FACEDOWN_DEFENCE,POS_FACEUP_DEFENCE,POS_FACEDOWN_DEFENCE)
	end
end

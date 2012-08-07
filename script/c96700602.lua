--波動障壁
function c96700602.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_POSITION+CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c96700602.condition)
	e1:SetCost(c96700602.cost)
	e1:SetTarget(c96700602.target)
	e1:SetOperation(c96700602.operation)
	c:RegisterEffect(e1)
end
function c96700602.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp
end
function c96700602.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,Card.IsType,1,nil,TYPE_SYNCHRO) end
	local g=Duel.SelectReleaseGroup(tp,Card.IsType,1,1,nil,TYPE_SYNCHRO)
	Duel.Release(g,REASON_COST)
end
function c96700602.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsAttackPos,tp,0,LOCATION_MZONE,1,nil) end
	local tc=eg:GetFirst()
	if tc:IsLocation(LOCATION_MZONE) then
		Duel.SetTargetCard(tc)
		Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,tc:GetDefence())
	end
end
function c96700602.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local g=Duel.GetMatchingGroup(Card.IsAttackPos,tp,0,LOCATION_MZONE,nil)
	if Duel.ChangePosition(g,POS_FACEUP_DEFENCE,POS_FACEDOWN_DEFENCE,0,0)~=0 then
		if tc:IsRelateToEffect(e) and tc:IsFaceup() then
			Duel.Damage(1-tp,tc:GetDefence(),REASON_EFFECT)
		end
	end
end

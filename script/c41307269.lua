--超重武者カブ－10
function c41307269.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(41307269,0))
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c41307269.condition)
	e1:SetTarget(c41307269.target)
	e1:SetOperation(c41307269.operation)
	c:RegisterEffect(e1)
end
function c41307269.cfilter(c,tp)
	return c:GetSummonPlayer()~=tp
end
function c41307269.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c41307269.cfilter,1,nil,tp)
end
function c41307269.filter(c)
	return c:IsPosition(POS_FACEUP_ATTACK) and c:IsSetCard(0x9a)
end
function c41307269.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c41307269.filter,tp,LOCATION_MZONE,0,1,nil) end
	local g=Duel.GetMatchingGroup(c41307269.filter,tp,LOCATION_MZONE,0,nil)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,g,g:GetCount(),0,0)
end
function c41307269.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c41307269.filter,tp,LOCATION_MZONE,0,nil)
	Duel.ChangePosition(g,POS_FACEUP_DEFENCE,POS_FACEDOWN_DEFENCE,0,0)
	local og=Duel.GetOperatedGroup()
	local tc=og:GetFirst()
	while tc do
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_DEFENCE)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetValue(500)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_END)
		tc:RegisterEffect(e1)
		tc=og:GetNext()
	end
end

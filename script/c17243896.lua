--グランド・スパイダー
function c17243896.initial_effect(c)
	--pos
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(17243896,0))
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1,EFFECT_COUNT_CODE_SINGLE)
	e1:SetCondition(c17243896.condition)
	e1:SetTarget(c17243896.target)
	e1:SetOperation(c17243896.operation)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e2)
end
function c17243896.filter(c,e,tp)
	return c:IsPosition(POS_FACEUP_ATTACK) and c:GetSummonPlayer()==1-tp and (not e or c:IsRelateToEffect(e))
end
function c17243896.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsPosition(POS_FACEUP_DEFENCE)
end
function c17243896.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c17243896.filter,1,nil,nil,tp) end
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,eg,eg:GetCount(),0,0)
end
function c17243896.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=eg:Filter(c17243896.filter,nil,e,tp)
	Duel.ChangePosition(g,POS_FACEUP_DEFENCE)
end

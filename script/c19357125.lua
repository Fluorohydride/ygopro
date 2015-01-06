--ダーク・アサシン
function c19357125.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetValue(c19357125.atkval)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(19357125,0))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c19357125.condition)
	e2:SetCost(c19357125.cost)
	e2:SetTarget(c19357125.target)
	e2:SetOperation(c19357125.operation)
	c:RegisterEffect(e2)
end
function c19357125.atkval(e,c)
	local ct=Duel.GetMatchingGroupCount(Card.IsAttribute,c:GetControler(),LOCATION_GRAVE,0,nil,ATTRIBUTE_DARK)
	if ct<=1 then return -400
	elseif ct<=4 then return 400
	else return 0 end
end
function c19357125.condition(e,tp,eg,ep,ev,re,r,rp)
	return not e:GetHandler():IsDisabled() and Duel.IsExistingMatchingCard(Card.IsAttribute,tp,LOCATION_GRAVE,0,5,nil,ATTRIBUTE_DARK)
end
function c19357125.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c19357125.filter(c)
	return c:IsFacedown() and c:IsDestructable()
end
function c19357125.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c19357125.filter,tp,0,LOCATION_MZONE,1,nil) end
	local g=Duel.GetMatchingGroup(c19357125.filter,tp,0,LOCATION_MZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c19357125.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c19357125.filter,tp,0,LOCATION_MZONE,nil)
	Duel.Destroy(g,REASON_EFFECT)
end

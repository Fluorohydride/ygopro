--白い泥棒
function c15150365.initial_effect(c)
	--handes
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(15150365,0))
	e1:SetCategory(CATEGORY_HANDES)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_DAMAGE)
	e1:SetCondition(c15150365.condition)
	e1:SetTarget(c15150365.target)
	e1:SetOperation(c15150365.operation)
	c:RegisterEffect(e1)
end
function c15150365.condition(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp and r==REASON_BATTLE and eg:GetFirst()==e:GetHandler()
end
function c15150365.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_HANDES,0,0,1-tp,1)
end
function c15150365.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetFieldGroup(ep,LOCATION_HAND,0,nil)
	local sg=g:RandomSelect(ep,1)
	Duel.SendtoGrave(sg,REASON_DISCARD+REASON_EFFECT)
end

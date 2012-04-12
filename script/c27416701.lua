--柴戦士タロ
function c27416701.initial_effect(c)
	--battle indestructable
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetValue(1)
	c:RegisterEffect(e1)
	--damage
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(27416701,0))
	e2:SetCategory(CATEGORY_TOHAND)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_DESTROYED)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c27416701.condition)
	e2:SetTarget(c27416701.target)
	e2:SetOperation(c27416701.operation)
	c:RegisterEffect(e2)
end
function c27416701.filter(c)
	return c:IsPreviousLocation(LOCATION_ONFIELD) and c:IsReason(REASON_BATTLE+REASON_EFFECT)
end
function c27416701.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c27416701.filter,1,nil)
end
function c27416701.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,e:GetHandler(),1,0,0)
end
function c27416701.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		Duel.SendtoHand(c,nil,REASON_EFFECT)
	end
end

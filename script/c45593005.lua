--D・マグネンI
function c45593005.initial_effect(c)
	--atk
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(45593005,0))
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c45593005.cona)
	e1:SetTarget(c45593005.tga)
	e1:SetOperation(c45593005.opa)
	c:RegisterEffect(e1)
	--def
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_MZONE)
	e2:SetTargetRange(LOCATION_MZONE,0)
	e2:SetCode(EFFECT_CANNOT_ATTACK)
	e2:SetCondition(c45593005.cond)
	c:RegisterEffect(e2)
end
function c45593005.cfilter(c)
	return c:IsFaceup() and c:IsAttackPos()
end
function c45593005.cona(e,tp,eg,ep,ev,re,r,rp)
	return not e:GetHandler():IsDisabled() and e:GetHandler():IsAttackPos()
		and Duel.GetMatchingGroupCount(c45593005.cfilter,tp,LOCATION_MZONE,0,e:GetHandler())==2
		and not Duel.IsExistingMatchingCard(Card.IsDefencePos,tp,LOCATION_MZONE,0,1,e:GetHandler())
end
function c45593005.tga(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local c=e:GetHandler()
	local g=Duel.GetMatchingGroup(Card.IsFaceup,tp,LOCATION_MZONE,0,c)
	Duel.SetTargetCard(g)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_ATTACK)
	e1:SetProperty(EFFECT_FLAG_OATH)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetTarget(c45593005.ftarget)
	e1:SetLabel(c:GetFieldID())
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c45593005.ftarget(e,c)
	return e:GetLabel()~=c:GetFieldID()
end
function c45593005.filter(c,e)
	return c:IsFaceup() and c:IsRelateToEffect(e)
end
function c45593005.opa(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local sg=g:Filter(c45593005.filter,nil,e)
	if sg:GetCount()==0 then return end
	local atk=sg:GetSum(Card.GetAttack)
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetValue(atk)
	e1:SetReset(RESET_EVENT+0x1ff0000+RESET_PHASE+PHASE_END)
	c:RegisterEffect(e1)
end
function c45593005.cond(e)
	return e:GetHandler():IsDefencePos()
end

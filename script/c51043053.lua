--防覇龍ヘリオスフィア
function c51043053.initial_effect(c)
	--
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_ATTACK_ANNOUNCE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(0,LOCATION_MZONE)
	e1:SetCondition(c51043053.atcon)
	c:RegisterEffect(e1)
	--lv change
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(51043053,0))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetCondition(c51043053.condition)
	e2:SetOperation(c51043053.operation)
	c:RegisterEffect(e2)
end
function c51043053.atcon(e)
	return Duel.GetFieldGroupCount(e:GetHandlerPlayer(),LOCATION_MZONE,0)==1
		and Duel.GetFieldGroupCount(e:GetHandlerPlayer(),0,LOCATION_HAND)<5
end
function c51043053.filter(c)
	return c:IsFaceup() and c:GetLevel()==8 and c:IsRace(RACE_DRAGON)
end
function c51043053.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetLevel()~=8
		and Duel.IsExistingMatchingCard(c51043053.filter,tp,LOCATION_MZONE,0,1,nil)
end
function c51043053.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CHANGE_LEVEL)
		e1:SetValue(8)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_END)
		c:RegisterEffect(e1)
	end
end

--マキシマム・シックス
function c30707994.initial_effect(c)
	--dice
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(30707994,0))
	e1:SetCategory(CATEGORY_DICE)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetCondition(c30707994.condition)
	e1:SetTarget(c30707994.target)
	e1:SetOperation(c30707994.operation)
	c:RegisterEffect(e1)
end
function c30707994.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_ADVANCE
end
function c30707994.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DICE,nil,0,tp,1)
end
function c30707994.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		local dice=Duel.TossDice(tp,1)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetProperty(EFFECT_FLAG_COPY_INHERIT)
		e1:SetReset(RESET_EVENT+0x1ff0000)
		e1:SetValue(dice*200)
		c:RegisterEffect(e1)
	end
end

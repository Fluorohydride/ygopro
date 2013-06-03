--BF－熱風のギブリ
function c26775203.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(26775203,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c26775203.condition)
	e1:SetTarget(c26775203.target)
	e1:SetOperation(c26775203.operation)
	c:RegisterEffect(e1)
	--spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(26775203,1))
	e2:SetCategory(CATEGORY_ATKCHANGE+CATEGORY_DEFCHANGE)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetCountLimit(1)
	e2:SetRange(LOCATION_MZONE)
	e2:SetOperation(c26775203.adchange)
	c:RegisterEffect(e2)
end
function c26775203.condition(e,tp,eg,ep,ev,re,r,rp)
	local at=Duel.GetAttacker()
	return at:GetControler()~=tp and Duel.GetAttackTarget()==nil
end
function c26775203.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then
		return Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
	end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,c,1,0,0)
end
function c26775203.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) then
		Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP)
	end
end
function c26775203.adchange(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local ba=c:GetBaseAttack()
	local bd=c:GetBaseDefence()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SET_BASE_ATTACK)
	e1:SetValue(bd)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_SET_BASE_DEFENCE)
	e2:SetValue(ba)
	e2:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	c:RegisterEffect(e2)
end

--神海竜ギシルノドン
function c76891401.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure2(c,nil,aux.NonTuner(c76891401.synfilter))
	c:EnableReviveLimit()
	--atk change
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(76891401,0))
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c76891401.atkcon)
	e1:SetOperation(c76891401.atkop)
	c:RegisterEffect(e1)
end
function c76891401.synfilter(c)
	return c:GetLevel()==3
end
function c76891401.filter(c)
	return c:IsLevelBelow(3) and c:IsPreviousLocation(LOCATION_ONFIELD) and c:IsPreviousPosition(POS_FACEUP)
end
function c76891401.atkcon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c76891401.filter,1,nil)
end
function c76891401.atkop(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SET_ATTACK_FINAL)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	e1:SetValue(3000)
	c:RegisterEffect(e1)
end

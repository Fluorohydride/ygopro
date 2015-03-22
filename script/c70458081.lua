--EMジンライノ
function c70458081.initial_effect(c)
	--cannot be battle target
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e1:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e1:SetTarget(c70458081.bttg)
	e1:SetValue(aux.imval1)
	c:RegisterEffect(e1)
	--destroy replace
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EFFECT_DESTROY_REPLACE)
	e2:SetRange(LOCATION_GRAVE)
	e2:SetTarget(c70458081.reptg)
	e2:SetValue(c70458081.repval)
	e2:SetOperation(c70458081.repop)
	c:RegisterEffect(e2)
end
function c70458081.bttg(e,c)
	return c:IsSetCard(0x9f) and c~=e:GetHandler()
end
function c70458081.repfilter(c,tp)
	return c:IsFaceup() and c:IsSetCard(0x9f) and not c:IsCode(70458081)
		and c:IsOnField() and c:IsControler(tp) and c:IsReason(REASON_EFFECT+REASON_BATTLE)
end
function c70458081.reptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemove() and eg:IsExists(c70458081.repfilter,1,nil,tp) end
	return Duel.SelectYesNo(tp,aux.Stringid(70458081,0))
end
function c70458081.repval(e,c)
	return c70458081.repfilter(c,e:GetHandlerPlayer())
end
function c70458081.repop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_EFFECT)
end

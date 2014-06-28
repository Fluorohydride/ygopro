--異次元の生還者
function c48092532.initial_effect(c)
	--removed
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_REMOVE)
	e1:SetCondition(c48092532.rmcon)
	e1:SetOperation(c48092532.rmop)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(48092532,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_PHASE+PHASE_END)
	e2:SetRange(LOCATION_REMOVED)
	e2:SetCondition(c48092532.condition)
	e2:SetTarget(c48092532.target)
	e2:SetOperation(c48092532.operation)
	c:RegisterEffect(e2)
end
function c48092532.rmcon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return c:IsFaceup() and c:IsPreviousPosition(POS_FACEUP)
		and c:IsPreviousLocation(LOCATION_ONFIELD) and c:GetPreviousControler()==tp
end
function c48092532.rmop(e,tp,eg,ep,ev,re,r,rp)
	e:GetHandler():RegisterFlagEffect(48092532,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end
function c48092532.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetFlagEffect(48092532)~=0
end
function c48092532.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetFlagEffect(48092533)==0 end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
	e:GetHandler():RegisterFlagEffect(48092533,RESET_PHASE+PHASE_END,0,1)
end
function c48092532.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) then
		if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then
			Duel.SendtoGrave(e:GetHandler(),REASON_EFFECT)
			return
		end
		Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP)
	end
end

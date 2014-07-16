--ギミック・パペット－ボム・エッグ
function c20032555.initial_effect(c)
	--effects
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetDescription(aux.Stringid(20032555,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1,20032555)
	e1:SetTarget(c20032555.efftg)
	e1:SetOperation(c20032555.effop)
	c:RegisterEffect(e1)
end
function c20032555.cfilter(c)
	return c:IsSetCard(0x83) and c:IsType(TYPE_MONSTER) and c:IsDiscardable()
end
function c20032555.efftg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c20032555.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c20032555.cfilter,1,1,REASON_COST+REASON_DISCARD)
	local opt=0
	if e:GetHandler():GetLevel()==8 then
		opt=Duel.SelectOption(tp,aux.Stringid(20032555,1))
	else
		opt=Duel.SelectOption(tp,aux.Stringid(20032555,1),aux.Stringid(20032555,2))
	end
	e:SetLabel(opt)
	if opt==0 then
		Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,800)
	end
end
function c20032555.effop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 then
		Duel.Damage(1-tp,800,REASON_EFFECT)
	else
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CHANGE_LEVEL)
		e1:SetValue(8)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		e:GetHandler():RegisterEffect(e1)
	end
end

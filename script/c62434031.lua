--森羅の花卉士 ナルサス
function c62434031.initial_effect(c)
	--deck check
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(62434031,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetTarget(c62434031.target)
	e1:SetOperation(c62434031.operation)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(62434031,1))
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e2:SetCondition(c62434031.tdcon)
	e2:SetTarget(c62434031.tdtg)
	e2:SetOperation(c62434031.tdop)
	c:RegisterEffect(e2)
	if not c62434031.global_check then
		c62434031.global_check=true
		c62434031[0]=Group.CreateGroup()
		c62434031[0]:KeepAlive()
		c62434031[1]=0
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_CONFIRM_DECKTOP)
		ge1:SetOperation(c62434031.checkop)
		Duel.RegisterEffect(ge1,0)
	end
end
function c62434031.checkop(e,tp,eg,ep,ev,re,r,rp)
	c62434031[0]:Clear()
	c62434031[0]:Merge(eg)
	c62434031[1]=re
end
function c62434031.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)>0 end
end
function c62434031.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)==0 then return end
	Duel.ConfirmDecktop(tp,1)
	local g=Duel.GetDecktopGroup(tp,1)
	local tc=g:GetFirst()
	if tc:IsRace(RACE_PLANT) then
		Duel.DisableShuffleCheck()
		Duel.SendtoGrave(g,REASON_EFFECT)
	else
		Duel.MoveSequence(tc,1)
	end
end
function c62434031.tdcon(e,tp,eg,ep,ev,re,r,rp)
	return re and e:GetHandler():IsPreviousLocation(LOCATION_DECK)
		and c62434031[0]:IsContains(e:GetHandler()) and c62434031[1]==re
end
function c62434031.tdtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsSetCard,tp,LOCATION_DECK,0,1,nil,0x90) end
end
function c62434031.tdop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(62434031,2))
	local g=Duel.SelectMatchingCard(tp,Card.IsSetCard,tp,LOCATION_DECK,0,1,1,nil,0x90)
	local tc=g:GetFirst()
	if tc then
		Duel.ShuffleDeck(tp)
		Duel.MoveSequence(tc,0)
		Duel.ConfirmDecktop(tp,1)
	end
end
